open Bobcat;

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
    ~render=Theia.hSeq,
    (),
  );

let transition1 =
  ConfigIR.mk(
    ~name="seq",
    ~nodes=[Some(str(~place="y", "y")), Some(str(~place="x", "x"))],
    ~render=Theia.hSeq,
    (),
  );

let delete1 =
  ConfigIR.mk(~name="seq", ~nodes=[Some(str(~place="y", "y"))], ~render=Theia.hSeq, ());

let nested0 =
  ConfigIR.mk(
    ~name="seq",
    ~nodes=[
      Some(str(~place="x", "x")),
      Some(
        ConfigIR.mk(
          ~name="y",
          ~nodes=[Some(str(~place="y0", "y0")), Some(str(~place="y1", "y1"))],
          ~render=Theia.hSeq,
          (),
        ),
      ),
    ],
    ~render=Theia.vSeq,
    (),
  );

/* NOTE! vSeq and hSeq intentionally switched to test complex animations. */
let nested1 =
  ConfigIR.mk(
    ~name="seq",
    ~nodes=[
      Some(str(~place="x", "x")),
      Some(
        ConfigIR.mk(~name="y", ~nodes=[Some(str(~place="y1", "y1"))], ~render=Theia.vSeq, ()),
      ),
    ],
    ~render=Theia.hSeq,
    (),
  );

let boxed0 =
  ConfigIR.mk(
    ~name="seq",
    ~nodes=[
      Some(
        ConfigIR.mk(
          ~place="box",
          ~name="box",
          ~nodes=[Some(str("x"))],
          ~render=([x]) => Theia.box(~dx=5., ~dy=5., x, []),
          (),
        ),
      ),
      Some(str(~place="y", "y")),
    ],
    ~render=Theia.hSeq,
    (),
  );

let boxed1 =
  ConfigIR.mk(
    ~name="seq",
    ~nodes=[
      Some(str(~place="y", "y")),
      Some(
        ConfigIR.mk(
          ~place="box",
          ~name="box",
          ~nodes=[Some(str("x"))],
          ~render=([x]) => Theia.box(~dx=5., ~dy=5., x, []),
          (),
        ),
      ),
    ],
    ~render=Theia.vSeq,
    (),
  );
