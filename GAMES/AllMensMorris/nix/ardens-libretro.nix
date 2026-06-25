{ lib
, stdenv
, fetchFromGitHub
, cmake
, ninja
}:

stdenv.mkDerivation rec {
  pname = "ardens-libretro";
  version = "0.24.15";

  src = fetchFromGitHub {
    owner = "tiberiusbrown";
    repo = "Ardens";
    rev = "v${version}";
    fetchSubmodules = true;
    hash = "sha256-pA5iCE2tg1/nyXwDoXGMBBTULahW5JbyfRT+R7KrYxQ=";
  };

  nativeBuildInputs = [
    cmake
    ninja
  ];

  cmakeFlags = [
    "-DARDENS_LIBRETRO=ON"
    "-DARDENS_LLVM=OFF"
    "-DARDENS_DEBUGGER=OFF"
    "-DARDENS_PLAYER=OFF"
    "-DARDENS_FLASHCART=OFF"
    "-DARDENS_DIST=OFF"
    "-DARDENS_BENCHMARK=OFF"
    "-DARDENS_CYCLES=OFF"
  ];

  installPhase = ''
    runHook preInstall

    install -Dm755 ardens_libretro.so \
      "$out/lib/libretro/ardens_libretro.so"

    runHook postInstall
  '';

  meta = {
    description = "Ardens Arduboy/Arduboy FX libretro core";
    homepage = "https://github.com/tiberiusbrown/Ardens";
    license = lib.licenses.mit;
    platforms = lib.platforms.linux;
  };
}
