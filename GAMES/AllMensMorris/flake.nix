{
  description = "All Men's Morris Arduboy game";

  inputs = {
    nixpkgs.url = "nixpkgs";
  };

  outputs = { self, nixpkgs }:
    let
      supportedSystems = [
        "x86_64-linux"
        "aarch64-linux"
      ];

      forAllSystems = nixpkgs.lib.genAttrs supportedSystems;
    in
    {
      packages = forAllSystems (system:
        let
          pkgs = import nixpkgs { inherit system; };
          ardens-libretro = pkgs.callPackage ./nix/ardens-libretro.nix { };
        in
        {
          inherit ardens-libretro;
          default = ardens-libretro;
        });

      devShells = forAllSystems (system:
        let
          pkgs = import nixpkgs { inherit system; };
          ardens-libretro = self.packages.${system}.ardens-libretro;
        in
        {
          default = pkgs.mkShell {
            packages = with pkgs; [
              arduino-cli
              ardens-libretro
              file
              gnumake
              git
              imagemagick
              pkgsCross.avr.buildPackages.binutils
              (python3.withPackages (python-pkgs: with python-pkgs; [
                pillow
                pyserial
              ]))
              retroarch
              simavr
              xdg-utils
            ];

            shellHook = ''
              export ARDUINO_DIRECTORIES_DATA="$PWD/.arduino"
              export ARDUINO_DIRECTORIES_USER="$PWD/.arduino-sketchbook"
              export ARDENS_LIBRETRO_CORE="${ardens-libretro}/lib/libretro/ardens_libretro.so"
              echo "All Men's Morris dev shell"
              echo "Run: make setup"
              echo "Then: make compile"
              echo "Local emulator: make libretro"
            '';
          };
        });

      formatter = forAllSystems (system:
        let
          pkgs = import nixpkgs { inherit system; };
        in
        pkgs.nixpkgs-fmt);
    };
}
