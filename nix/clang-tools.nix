{
  stdenv,
  lib,
  runCommand,
  makeWrapper,
  llvmPackages,
  extraQueryDriver ? "",
}:
let
  queryDriver = lib.concatStringsSep "," [
    "${stdenv.cc}/bin/*"
    extraQueryDriver
  ];
in
runCommand "clang-tools"
  {
    unwrapped = llvmPackages.clang-unwrapped;
    buildInputs = [ makeWrapper ];
  }
  ''
    mkdir -p $out/bin

    for tool in $unwrapped/bin/clang-*; do
      tool=$(basename "$tool")

      if [[ $tool == "clang-cl" || $tool == "clang-cpp" ]]; then
        continue
      fi

      if [[ ! $tool =~ ^clang\-[a-zA-Z_\-]+$ ]]; then
        continue
      fi

      ln -s $unwrapped/bin/$tool $out/bin/$tool
    done

    makeWrapper $unwrapped/bin/clangd $out/bin/clangd \
      --add-flags "--query-driver=${queryDriver}"     \
      --set LANG "en_US.UTF-8"
  ''
