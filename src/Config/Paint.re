/* fills in flow in kernel nodes that were untagged. similar to propagate but at the Kernel IR level */
let paint = (flow: Flow.linear, n: KernelIR.node(ConfigIR.kernelPlace)) => {
  let flowState = ref(flow);
  let rec paintAux =
          (parentTag, nodes: list(KernelIR.node(ConfigIR.kernelPlace)))
          : list(KernelIR.node(ConfigIR.kernelPlace)) => {
    List.mapi(
      (i, KernelIR.{tag, nodes} as n) => {
        let tag =
          switch (tag) {
          /* unpainted: inherit from parent, add to flowState if necessary */
          | None =>
            switch (parentTag) {
            | None => None
            | Some(None) => Some(None)
            | Some(Some(p)) =>
              switch (List.assoc_opt(p, flowState^)) {
              | Some(pDests) =>
                flowState :=
                  [(p, List.map(p => p ++ "p" ++ string_of_int(i), pDests)), ...flowState^]
              | None => ()
              };
              Some(Some(p ++ "p" ++ string_of_int(i)));
            }
          | Some(tag) => Some(tag)
          };

        let nodes = paintAux(tag, nodes);
        {...n, tag, nodes};
      },
      nodes,
    );
  };
  /* sequencing for flowState mutation */
  let n = {...n, nodes: paintAux(n.tag, n.nodes)};
  (flowState^, n);
};
