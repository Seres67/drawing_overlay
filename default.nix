{
  lib,
  llvmPackages_20,
  cmake,
  sdl3,
  sdl3-ttf,
  sdl3-image,
}:

llvmPackages_20.stdenv.mkDerivation rec {
  pname = "drawing-overlay";
  version = "0.1.0";

  src = ./.;

  nativeBuildInputs = [ cmake ];
  buildInputs = [
    sdl3
    sdl3-ttf
    sdl3-image
  ];

  installPhase = ''
    mkdir -p $out/bin
    cp drawing_overlay $out/bin
    cp ${./assets}/font.ttf $out/bin
  '';

  meta = with lib; {
    homepage = "https://github.com/Seres67/drawing-overlay";
    description = ''
      A drawing overlay to draw over your screen.
    '';
    license = licenses.mit;
    platforms = with platforms; linux;
    maintainers = [ "Seres67" ];
  };
}
