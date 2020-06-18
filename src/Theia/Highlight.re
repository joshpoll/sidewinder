module MS = Belt.Map.String;

let mk = (~uid=?, ~fill, node, links) => {
  let render = (nodes, bbox, links) => {
    <>
      <rect
        x={Js.Float.toString(bbox->Rectangle.x1)}
        y={Js.Float.toString(bbox->Rectangle.y1)}
        width={Js.Float.toString(bbox->Rectangle.width)}
        height={Js.Float.toString(bbox->Rectangle.height)}
        fill
      />
      {Kernel.defaultRender(nodes, links)}
    </>;
  };
  KernelIR.mk(
    ~uid?,
    ~nodes=[node],
    ~links,
    ~layout=Kernel.defaultLayout,
    ~computeBBox=bs => bs->MS.valuesToArray->Array.to_list->Rectangle.union_list,
    ~render,
  );
};