/* fills in flow in kernel nodes that were untagged. similar to propagate but at the Kernel IR level */
let convert = (flow: Flow.linear, n: Bobcat.KernelIR.node(ConfigIR.kernelPlace)) => {
  let flowState = ref(flow);
  let rec convertAux =
          (parentTag, nodes: list(Bobcat.KernelIR.node(ConfigIR.kernelPlace)))
          : list(Bobcat.KernelIR.node(ConfigIR.kernelPlace)) => {
    List.mapi(
      (i, Bobcat.KernelIR.{tag, nodes} as n) => {
        let tag =
          switch (tag) {
          /* unpainted: inherit from parent, add to flowState if necessary */
          | None =>
            switch (parentTag) {
            | None => None
            | Some([]) => Some([])
            | Some([p, ..._]) =>
              let place = p ++ "p" ++ string_of_int(i);
              switch (List.assoc_opt(p, flowState^)) {
              | Some(pDests) =>
                flowState :=
                  [(place, List.map(p => p ++ "p" ++ string_of_int(i), pDests)), ...flowState^]
              | None => ()
              };
              Some([place]);
            }
          | Some(tag) => Some(tag)
          };

        let nodes = convertAux(tag, nodes);
        {...n, tag, nodes};
      },
      nodes,
    );
  };
  /* sequencing for flowState mutation */
  let n = {...n, nodes: convertAux(n.tag, n.nodes)};
  (flowState^, n);
};
