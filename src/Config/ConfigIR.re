/* None = no place, Some(place) = place. no tag = unpainted */
type kernelPlace = option(Place.t);

/* TODO: add links field. may also need to be an input to rendering */
type node = {
  /* place is essentially a variable tag. in input config, only occurs at "leaves" i.e. no subnode of a node with a
     place can contain another place. however, after customization, this is not guaranteed. while
     useful for the frontend, not sure if that guarantee helps us internally...

     I think it will be useful, because we can push place information down to all subnodes of the
     labelled node when its input. Then place becomes read-only afterwards. Remains to be seen if that
     is a good strategy or not.
      */
  place: option(Place.t), /* TODO: should every node have a place? should this just be a uid? */
  /* essentially an ADT constructor ID e.g. bind, ctxt. NOT unique. */
  name: string,
  nodes: list(option(node)), /* None is Leaf, Some is internal Node. Rewrites can change these positions. */
  renderHole: KernelIR.node(kernelPlace),
  render: list(KernelIR.node(kernelPlace)) => KernelIR.node(kernelPlace),
};

let mk = (~place=?, ~renderHole=?, ~name, ~nodes, ~render, ()) => {
  place,
  name,
  nodes,
  renderHole: Belt.Option.getWithDefault(renderHole, Theia.hole(~tag=None, ())),
  render,
};
