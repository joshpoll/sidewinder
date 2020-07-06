/* fills in flow in kernel nodes that were untagged. similar to propagate but at the Kernel IR level */
let convert = (flow: Flow.linearExt, n: Bobcat.KernelIR.node(ConfigIR.kernelPlace)) => {
  let flowState = ref(flow);
  let rec convertAux =
          (
            parentTag: option(ConfigIR.kernelPlace),
            nodes: list(Bobcat.KernelIR.node(ConfigIR.kernelPlace)),
          )
          : list(Bobcat.KernelIR.node(ConfigIR.kernelPlace)) => {
    List.mapi(
      (i, Bobcat.KernelIR.{tag, nodes} as n: Bobcat.KernelIR.node(ConfigIR.kernelPlace)) => {
        let tag =
          switch (parentTag, tag) {
          | (_, Some(tag)) => Some(tag)
          /* unpainted: inherit from parent, add to flowState if necessary */
          | (None, None)
          | (Some({pat: None, extFns: _}), None) => parentTag
          | (Some({pat: Some(pat), extFns: rest}), None) =>
            let place = pat ++ "p" ++ string_of_int(i);
            switch (List.assoc_opt(pat, flowState^.pattern)) {
            | Some(pDests) =>
              flowState :=
                {
                  ...flowState^,
                  pattern: [
                    (place, List.map(p => p ++ "p" ++ string_of_int(i), pDests)),
                    ...flowState^.pattern,
                  ],
                }
            | None => ()
            };
            Some(ConfigGraphIR.{pat: Some(place), extFns: rest});
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
