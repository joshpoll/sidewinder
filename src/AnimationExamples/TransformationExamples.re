open Bobcat;

let add =
  ConfigIR.mk(
    ~place="add",
    ~name="add",
    ~nodes=[None, None],
    ~render=
      ([x, y]) =>
        Theia.hSeq(
          ~gap=2.,
          [
            Theia.str("("),
            x,
            Theia.str(")"),
            Theia.str("+"),
            Theia.str("("),
            y,
            Theia.str(")"),
          ],
        ),
    (),
  );
