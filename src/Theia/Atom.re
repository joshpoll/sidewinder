module MS = Belt.Map.String;

let mk = (~uid=?, ~tag=?, ~links=[], r, sizeOffset) =>
  KernelIR.mk(
    ~uid?,
    ~tag?,
    ~nodes=[],
    ~links,
    ~layout=(_, _) => MS.empty,
    ~computeBBox=_ => sizeOffset,
    ~nodeRender=_ => r,
  );