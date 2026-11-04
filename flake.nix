{
  description = "TUILIB";

  inputs = {
    flake-utils.url = "github:numtide/flake-utils";
  };

  outputs = { self, nixpkgs, flake-utils }: {
    overlays.default = final: prev: {
      TUILIB = final.callPackage ({stdenv, pkgs, ...}:
        stdenv.mkDerivation {
          src = ./.;
          pname = "TUILIB";
          version = "unstable";
          nativeBuildInputs = with pkgs; [  ];
          buildInputs = with pkgs; [  ];
          
          installPhase = ''
            mkdir -p $out/bin
            cp $pname $out/bin/TUILIB
          '';
        }
      ) {};
    };
  } // 
  (flake-utils.lib.eachDefaultSystem (system:
    let
      pkgs = import nixpkgs {
        system = system;
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

          '';
        };
      }
  ));
}

