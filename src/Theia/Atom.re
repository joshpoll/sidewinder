module MS = Belt.Map.String;

let mk = (~uid=?, ~links=[], r, sizeOffset) =>
  KernelIR.mk(
    ~uid?,
    ~nodes=[],
    ~links,
    ~layout=(_, _) => MS.empty,
    ~computeBBox=_ => sizeOffset,
    ~nodeRender=_ => r,
  );