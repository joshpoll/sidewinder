let mk = (~uid=?, ~gap=0., nodes) =>
  Seq.mk(~uid?, ~nodes, ~linkRender=None, ~gap, ~direction=LeftRight);