// Pocket Stage - the game's frames and sound to a video, with what macOS
// brings along (no download needed). tools/post_media.py calls this.
//
//   swift tools/video.swift <frames> <sound.pcm> <out.mp4> [scale] [width height] [bits a second]
//
// frames: 1024 bytes a frame (128 x 64, a bit a pixel, rows of 16 bytes, the
// left pixel in the top bit), one frame every 16 ms (62.5 a second, the
// game's own pace). sound: 16-bit mono at 50,000 samples a second (the
// emulator's rate), or "-" for none; it is converted to 48,000 for AAC.
// Each pixel becomes a scale x scale square (8 by default: the squares stay
// on the encoder's 8 x 8 blocks, so the edges stay sharp), centred on a
// black picture of width x height (1280 x 720 by default).
import AVFoundation
import CoreVideo
import Foundation

let args = CommandLine.arguments
guard args.count >= 4 else {
  print("usage: video <frames> <sound.pcm|-> <out.mp4> [scale] [width height] [bits a second]")
  exit(1)
}
let scale = args.count > 4 ? Int(args[4])! : 8
let width = args.count > 6 ? Int(args[5])! : 1280
let height = args.count > 6 ? Int(args[6])! : 720
let bits = args.count > 7 ? Int(args[7])! : 6_000_000
let frames = try! Data(contentsOf: URL(fileURLWithPath: args[1]))
let count = frames.count / 1024
let out = URL(fileURLWithPath: args[3])
let left = (width - 128 * scale) / 2, top = (height - 64 * scale) / 2

try? FileManager.default.removeItem(at: out)
let writer = try! AVAssetWriter(outputURL: out, fileType: .mp4)
let video = AVAssetWriterInput(mediaType: .video, outputSettings: [
  AVVideoCodecKey: AVVideoCodecType.h264,
  AVVideoWidthKey: width,
  AVVideoHeightKey: height,
  AVVideoCompressionPropertiesKey: [
    AVVideoAverageBitRateKey: bits,
    AVVideoProfileLevelKey: AVVideoProfileLevelH264HighAutoLevel,
    AVVideoMaxKeyFrameIntervalKey: 125,
  ],
])
video.expectsMediaDataInRealTime = false
let pixels = AVAssetWriterInputPixelBufferAdaptor(assetWriterInput: video, sourcePixelBufferAttributes: [
  kCVPixelBufferPixelFormatTypeKey as String: kCVPixelFormatType_32BGRA,
  kCVPixelBufferWidthKey as String: width,
  kCVPixelBufferHeightKey as String: height,
])
writer.add(video)

// the sound: 50,000 a second, converted to 48,000 by Apple's converter
var audio: AVAssetWriterInput? = nil
var pcm48: AVAudioPCMBuffer? = nil
if args[2] != "-" {
  let raw = try! Data(contentsOf: URL(fileURLWithPath: args[2]))
  let n = raw.count / 2
  let inFormat = AVAudioFormat(commonFormat: .pcmFormatFloat32, sampleRate: 50_000, channels: 1, interleaved: false)!
  let outFormat = AVAudioFormat(commonFormat: .pcmFormatFloat32, sampleRate: 48_000, channels: 1, interleaved: false)!
  let src = AVAudioPCMBuffer(pcmFormat: inFormat, frameCapacity: AVAudioFrameCount(n))!
  src.frameLength = AVAudioFrameCount(n)
  raw.withUnsafeBytes { (p: UnsafeRawBufferPointer) in
    let s = p.bindMemory(to: Int16.self)
    let d = src.floatChannelData![0]
    for i in 0..<n { d[i] = Float(Int16(littleEndian: s[i])) / 32768 }
  }
  let conv = AVAudioConverter(from: inFormat, to: outFormat)!
  conv.sampleRateConverterQuality = AVAudioQuality.max.rawValue
  let dst = AVAudioPCMBuffer(pcmFormat: outFormat, frameCapacity: AVAudioFrameCount(Double(n) * 0.96) + 4096)!
  var given = false
  var error: NSError?
  conv.convert(to: dst, error: &error) { _, status in
    if given { status.pointee = .endOfStream; return nil }
    given = true
    status.pointee = .haveData
    return src
  }
  if let error = error { print("sound: \(error)"); exit(1) }
  pcm48 = dst
  let a = AVAssetWriterInput(mediaType: .audio, outputSettings: [
    AVFormatIDKey: kAudioFormatMPEG4AAC,
    AVSampleRateKey: 48_000,
    AVNumberOfChannelsKey: 1,
    AVEncoderBitRateKey: 160_000,
  ])
  a.expectsMediaDataInRealTime = false
  writer.add(a)
  audio = a
}

writer.startWriting()
writer.startSession(atSourceTime: .zero)

// a picture: the frame's pixels as squares on black
let white: UInt32 = 0xFFFF_FFFF, black: UInt32 = 0xFF00_0000
func appendPicture(_ k: Int) {
  var buffer: CVPixelBuffer?
  CVPixelBufferPoolCreatePixelBuffer(nil, pixels.pixelBufferPool!, &buffer)
  let pb = buffer!
  CVPixelBufferLockBaseAddress(pb, [])
  let base = CVPixelBufferGetBaseAddress(pb)!
  let stride = CVPixelBufferGetBytesPerRow(pb) / 4
  let px = base.bindMemory(to: UInt32.self, capacity: stride * height)
  for i in 0..<(stride * height) { px[i] = black }
  frames.withUnsafeBytes { (f: UnsafeRawBufferPointer) in
    for y in 0..<64 {
      for x in 0..<128 {
        if f[k * 1024 + y * 16 + x / 8] & UInt8(0x80 >> (x % 8)) == 0 { continue }
        for dy in 0..<scale {
          let row = (top + y * scale + dy) * stride + left + x * scale
          for dx in 0..<scale { px[row + dx] = white }
        }
      }
    }
  }
  CVPixelBufferUnlockBaseAddress(pb, [])
  pixels.append(pb, withPresentationTime: CMTime(value: CMTimeValue(k * 16), timescale: 1000))
}

// a piece of the sound: n samples from at
var soundFormat: CMAudioFormatDescription?
if let p = pcm48 {
  CMAudioFormatDescriptionCreate(allocator: nil, asbd: p.format.streamDescription, layoutSize: 0, layout: nil,
                                 magicCookieSize: 0, magicCookie: nil, extensions: nil,
                                 formatDescriptionOut: &soundFormat)
}
func appendSound(_ a: AVAssetWriterInput, _ p: AVAudioPCMBuffer, _ at: Int, _ n: Int) {
  var block: CMBlockBuffer?
  CMBlockBufferCreateWithMemoryBlock(allocator: nil, memoryBlock: nil, blockLength: n * 4, blockAllocator: nil,
                                     customBlockSource: nil, offsetToData: 0, dataLength: n * 4, flags: 0,
                                     blockBufferOut: &block)
  CMBlockBufferReplaceDataBytes(with: p.floatChannelData![0] + at, blockBuffer: block!, offsetIntoDestination: 0,
                                dataLength: n * 4)
  var sample: CMSampleBuffer?
  CMAudioSampleBufferCreateReadyWithPacketDescriptions(allocator: nil, dataBuffer: block!,
                                                       formatDescription: soundFormat!, sampleCount: n,
                                                       presentationTimeStamp: CMTime(value: CMTimeValue(at),
                                                                                     timescale: 48_000),
                                                       packetDescriptions: nil, sampleBufferOut: &sample)
  a.append(sample!)
}

// pictures and sound in the order of their times: the writer interleaves the
// two and will not take one far ahead of the other (it waits, for ever)
let total = pcm48.map { Int($0.frameLength) } ?? 0
var k = 0, at = 0
while k < count || at < total {
  let pictureTime = Double(k) * 0.016, soundTime = Double(at) / 48_000
  if k < count && (at >= total || pictureTime <= soundTime) {
    while !video.isReadyForMoreMediaData { usleep(500) }
    appendPicture(k)
    k += 1
  } else {
    let a = audio!
    while !a.isReadyForMoreMediaData { usleep(500) }
    let n = min(4096, total - at)
    appendSound(a, pcm48!, at, n)
    at += n
  }
}
video.markAsFinished()
audio?.markAsFinished()

let done = DispatchSemaphore(value: 0)
writer.finishWriting { done.signal() }
done.wait()
if writer.status != .completed {
  print("failed: \(writer.error?.localizedDescription ?? "?")")
  exit(1)
}
let size = (try? FileManager.default.attributesOfItem(atPath: out.path)[.size] as? Int) ?? 0
print(String(format: "wrote %@: %d frames, %.1f s, %.1f MB", out.lastPathComponent, count, Double(count) * 0.016,
             Double(size) / 1_048_576))
