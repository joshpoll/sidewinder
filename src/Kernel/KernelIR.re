module MS = Belt.Map.String;

type node = {
  uid: UID.t,
  nodes: list(node),
  links: list(Link.t),
  layout: (list((UID.t, Node.bbox)), list(Link.layout)) => MS.t(Node.transform),
  computeBBox: MS.t(Node.bbox) => Node.bbox,
  nodeRender: Node.bbox => React.element,
};

let mk = (~uid=?, ~nodes, ~links, ~layout, ~computeBBox, ~nodeRender) => {
  uid:
    switch (uid) {
    | None => "autogen__" ++ string_of_int(UID.mk())
    | Some(uid) => uid
    },
  nodes,
  links,
  layout,
  computeBBox,
  nodeRender,
};