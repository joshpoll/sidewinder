/* TODO: need to figure out a better way to do this. I also think holes might crash the program. */
let orHole = on =>
  switch (on) {
  | None => Theia.hole()
  | Some(n) => n
  };

let str = (~place=?, s) =>
  ConfigIR.mk(~place?, ~name=s, ~nodes=[], ~render=_ => Theia.str(s), ());

let transition0 =
  ConfigIR.mk(
    ~name="seq",
    ~nodes=[Some(str(~place="x", "x")), Some(str(~place="y", "y"))],
    ~render=([x, y]) => Theia.hSeq([orHole(x), orHole(y)]),
    (),
  );

let transition1 =
  ConfigIR.mk(
    ~name="seq",
    ~nodes=[Some(str(~place="y", "y")), Some(str(~place="x", "x"))],
    ~render=([y, x]) => Theia.hSeq([orHole(y), orHole(x)]),
    (),
  );

let delete1 =
  ConfigIR.mk(
    ~name="seq",
    ~nodes=[Some(str(~place="y", "y"))],
    ~render=([y]) => Theia.hSeq([orHole(y)]),
    (),
  );