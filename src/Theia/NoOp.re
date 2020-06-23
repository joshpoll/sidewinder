module MS = Belt.Map.String;

let mk = (~uid=?, node, links) => {
  KernelIR.mk(
    ~uid?,
    ~nodes=[node],
    ~links,
    ~layout=Kernel.defaultLayout,
    ~computeBBox=bs => bs->MS.valuesToArray->Array.to_list->Rectangle.union_list,
    ~nodeRender=_ => React.null,
  );
};