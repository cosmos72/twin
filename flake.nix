{
  description = "Nix flake for twin";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
  };

  outputs = { self, nixpkgs }:
    let
      lib = nixpkgs.lib;
      systems = [
        "x86_64-linux"
        "aarch64-linux"
      ];
      forAllSystems = f: lib.genAttrs systems (system: f system);
    in
    {
      packages = forAllSystems (system:
        let
          pkgs = import nixpkgs { inherit system; };

          mkTwin = {
            withX11 ? true,
          }:
            pkgs.stdenv.mkDerivation {
              pname = if withX11 then "twin" else "twin-no-x11";
              version = "1.0.0";

              src = self;

              nativeBuildInputs = with pkgs; [
                pkg-config
              ];

              buildInputs = with pkgs;
                [
                  gpm
                  ncurses
                  zlib
                ]
                ++ lib.optionals withX11 [
                  fontconfig
                  freetype
                  xorg.libX11
                  xorg.libXft
                ];

              configureFlags =
                [
                  "--enable-server-static=no"
                ]
                ++ lib.optionals (!withX11) [
                  "--enable-hw-x11=no"
                  "--enable-hw-xft=no"
                ];

              enableParallelBuilding = true;

              meta = with pkgs.lib; {
                description = "Textmode window environment with terminal, X11, and nested display backends";
                homepage = "https://github.com/cosmos72/twin";
                license = licenses.gpl2Plus;
                mainProgram = "twin";
                platforms = platforms.linux;
              };
            };

          twin = mkTwin { withX11 = true; };
          twinNoX = mkTwin { withX11 = false; };
        in
        {
          default = twin;
          twin = twin;
          "twin-nox" = twinNoX;
        });

      devShells = forAllSystems (system:
        let
          pkgs = import nixpkgs { inherit system; };
        in
        {
          default = pkgs.mkShell {
            inputsFrom = [
              self.packages.${system}.default
            ];

            nativeBuildInputs = with pkgs; [
              autoconf
              automake
              libtool
              pkg-config
            ];
          };
        });
    };
}
