{
  stdenv,
  lib,

  # Build tools
  meson,
  ninja,
  pkg-config,
}:
stdenv.mkDerivation {
  pname = "fp";
  version = "0.1.0";
  src = lib.cleanSource ./.;

  nativeBuildInputs = [
    meson
    ninja
    pkg-config
  ];
}
