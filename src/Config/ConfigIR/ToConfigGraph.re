let rec lowerOption = (on: option(ConfigIR.node)): option(ConfigGraphIR.node) =>
  switch (on) {
  | None => None
  | Some({place, name, nodes, renderHole, render}) =>
    let nodes = List.map(lowerOption, nodes);
    let pat = place;
    Some({
      place: {
        pat,
        extFns: [],
      },
      name,
      nodes,
      links: (),
      renderHole,
      render,
    });
  };

let lower = (n: ConfigIR.node): ConfigGraphIR.node => lowerOption(Some(n))->Belt.Option.getExn;
