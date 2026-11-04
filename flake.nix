{
  description = "TUILIB";

  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs/nixpkgs-unstable";
    flake-utils.url = "github:numtide/flake-utils";
  };

  outputs = { self, nixpkgs, flake-utils }: {
    overlays.default = final: prev: {
      TUILIB = final.callPackage ({stdenv, ...}:
        stdenv.mkDerivation {
          src = ./.;
          pname = "TUILIB";
          version = "0.1.0";
          nativeBuildInputs = [ stdenv.cc ];
          
          buildPhase = ''
            runHook preBuild

            $CC -c src/*.c -Iinclude -O3 -fPIC
            ar rcs libtuilib.a *.o

            runHook postBuild
          '';

          installPhase = ''
            runHook preInstall

            mkdir -p $out/include $out/lib

            cp include/*.h $out/include/
            cp libtuilib.a $out/lib/

            runHook postInstall          
          '';
        }
      ) {};
    };
  } // 
  (flake-utils.lib.eachDefaultSystem (system:
    let
      pkgs = import nixpkgs {
        inherit system;
        overlays = [ self.overlays.default ];
      };
      lib = nixpkgs.lib;
    in
      {
        packages = {
          TUILIB = pkgs.TUILIB;
          default = self.packages.${system}.TUILIB;
        };

        devShells.default = pkgs.mkShell {
          buildInputs = with pkgs; [  ];
          shellHook = ''
            alias build="cc -c src/*.c -Iinclude -O3 -fPIC; ar rcs libtuilib.a *.o"
            alias comp="cc main.c -Iinclude -L. -ltuilib -o test"
            alias run="./test"
          '';
        };
      }
  ));
}

