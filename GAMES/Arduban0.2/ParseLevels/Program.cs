using System;
using System.Collections.Generic;
using System.Linq;
using System.IO;

namespace ParseLevels
{
    class Program
    {
        static Dictionary<char, byte> TileMap { get; } = CreateTileMap();
        static int Count { get; set; } = 0;

        static void Main(string[] args)
        {
            if(args.Length == 0)
            {
                Console.WriteLine("Usage: ParseLevels <filename> <filename>");
                return;
            }

            Console.WriteLine("#ifndef LEVELS_H");
            Console.WriteLine("#define LEVELS_H");
            Console.WriteLine();
            Console.WriteLine("#include \"globals.h\"");
            Console.WriteLine();
            Console.WriteLine("const byte Levels[] PROGMEM = {");

            foreach(string arg in args)
            {
                if(!File.Exists(arg))
                {
                    Console.WriteLine($"// Level file {arg} not found");
                    continue;
                }

                var pack = Path.GetFileNameWithoutExtension(arg);
                string name = null;
                List<string> puzzle = new List<string>();

                using(var file = new System.IO.StreamReader(arg))
                {
                    try
                    {
                        string line = file.ReadLine();
                        while(line != null)
                        {
                            if(line.StartsWith(";"))
                            {
                                name = line.Substring(2);
                                puzzle.Clear();
                            }
                            else if(string.IsNullOrWhiteSpace(line))
                            {
                                if(puzzle.Count > 0)
                                {
                                    ParsePuzzle(pack, name, puzzle);
                                    puzzle.Clear();
                                }
                            }
                            else
                            {
                                puzzle.Add(line);
                            }
                            line = file.ReadLine();
                        }

                        if(puzzle.Count > 0)
                        {
                            ParsePuzzle(pack, name, puzzle);
                        }
                    }
                    catch(Exception ex)
                    {
                        Console.Error.WriteLine($"Error processing file {arg}");
                        Console.Error.WriteLine(ex);
                    }
                }
            }
            Console.WriteLine("};");
            Console.WriteLine();
            Console.WriteLine("#endif");
        }

        static void ParsePuzzle(string pack, string name, List<string> puzzle)
        {
            byte[,] level = GetLevel(puzzle);
            if(level != null)
            {
                List<List<byte>> rle = RunLengthEncodeLevel(level);
                OutputLevel(pack, name, rle);
            }
        }

        /// <summary>Outputs one level to the console</summary
        static void OutputLevel(string pack, string name, List<List<byte>> rle)
        {
            Console.WriteLine($"    // Level {++Count} from {pack} level {name}");
            foreach(List<byte> row in rle)
            {
                Console.Write("    ");
                foreach(byte b in row)
                {
                    Console.Write($"0x{b:X2}, ");
                }
                Console.WriteLine();
            }
        }

        static byte[,] GetLevel(List<string> puzzle)
        {
            int width = longestLine(puzzle);
            int height = puzzle.Count;

            if(width > 16 || height > 16)
                return null;

            if(width > 8 && height > 8)
                return null;

            // Switch to landscape mode for the arduboy wide screen
            bool rotate = height > width;

            byte[,] level = new byte[8,16];
            for(int row = 0; row < 8; row++)
            {
                for(int col = 0; col < 16; col++)
                {
                    level[row,col] = 0x07;  // FLOOR
                }
            }

            // Center the puzzles
            int r_off = rotate ? (8 - width) / 2 : (8 - height) / 2;
            int c_off = rotate ? (16 - height) / 2 : (16 - width) / 2;

            int r = 0;
            foreach(string row in puzzle)
            {
                for(int c = 0; c < row.Length; c++)
                {
                    if(rotate)
                        level[c + r_off, r + c_off] = TileMap[row[c]];
                    else
                        level[r + r_off, c + c_off] = TileMap[row[c]];
                }
                r++;
            }
            return level;
        }

        static List<List<byte>> RunLengthEncodeLevel(byte[,] level)
        {
            List<List<byte>> rle = new List<List<byte>>(8);

            for(int row = 0; row < 8; row++)
            {
                var list = new List<byte>();
                rle.Add(list);
                byte last = level[row, 0];
                byte count = 0; // Count is zero based
                for(int col = 1; col < 16; col++)
                {
                    if(level[row, col] != last)
                    {
                        list.Add((byte)(((int)count << 4) | (int)last));
                        last = level[row, col];
                        count = 0;
                    }
                    else
                    {
                        count++;
                    }
                }
                list.Add((byte)(((int)count << 4) | (int)last));
            }
            byte byteCount = (byte)rle.Sum(row => row.Count());
            rle.Insert(0, new List<byte> { byteCount });
            return rle;
        }

        static int longestLine(IEnumerable<string> puzzle) =>
            puzzle.Max(s => s.Length);

        static Dictionary<char, byte> CreateTileMap() =>
            new Dictionary<char, byte>
            {
                {'#', 0x01},    // WALL
                {'@', 0x02},    // PLAYER
                {'+', 0x03},    // PLAYER_ON_GOAL
                {'$', 0x04},    // BOX
                {'*', 0x05},    // BOX_ON_GOAL
                {'.', 0x06},    // GOAL
                {' ', 0x07}     // FLOOR
            };
    }
}
