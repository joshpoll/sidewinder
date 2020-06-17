module MS = Belt.Map.String;

/* TODO: this needs to accept a layout parameter probably. Ideally box should be able to call this.
   But if I add that as a parameter this function is the same as Sidewinder.make */
let mk = (~uid=?, ~nodes, ~links, ~computeBBox, ~render) =>
  KernelIR.mk(~uid?, ~nodes, ~links, ~layout=KernelIR.defaultLayout, ~computeBBox, ~render);