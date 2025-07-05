{ pkgs }:

pkgs.mkShell rec {
  name = "drawing-overlay";

  packages = with pkgs; [
    llvmPackages_20.clang
    cmake
    sdl3
    sdl3-ttf
    sdl3-image
  ];

  shellHook =
    let
      icon = "f121";
    in
    ''
      export PS1="$(echo -e '\u${icon}') {\[$(tput sgr0)\]\[\033[38;5;228m\]\w\[$(tput sgr0)\]\[\033[38;5;15m\]} (${name}) \\$ \[$(tput sgr0)\]"
    '';
}
