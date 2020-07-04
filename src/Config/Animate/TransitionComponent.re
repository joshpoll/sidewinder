open SpringHelper;
open Bobcat;

module SpringHook =
  Spring.MakeSpring({
    type t = (float, float, float, float);
    type interpolate = (float, float, float, float) => string;
  });

let computeSVGTransform =
    ({translate: {x: tx, y: ty}, scale: {x: sx, y: sy}}: Bobcat.Node.transform, bbox) => {
  /* https://css-tricks.com/transforms-on-svg-elements/ */
  let scale =
    "translate("
    ++ Js.Float.toString(bbox->Rectangle.x1 +. bbox->Rectangle.width /. 2.)
    ++ ", "
    ++ Js.Float.toString(bbox->Rectangle.y1 +. bbox->Rectangle.height /. 2.)
    ++ ")";
  let scale = scale ++ " " ++ {j|scale($sx, $sy)|j};
  let scale =
    scale
    ++ " "
    ++ "translate("
    ++ Js.Float.toString(-. (bbox->Rectangle.x1 +. bbox->Rectangle.width /. 2.))
    ++ ", "
    ++ Js.Float.toString(-. (bbox->Rectangle.y1 +. bbox->Rectangle.height /. 2.))
    ++ ")";

  let translate = {j|translate($tx, $ty)|j};

  scale ++ " " ++ translate;
};

let computeSVGTransformFlattened = (bbox, tx, ty, sx, sy) =>
  computeSVGTransform({
                        translate: {
                          x: tx,
                          y: ty,
                        },
                        scale: {
                          x: sx,
                          y: sy,
                        },
                      }, bbox);

[@react.component]
let make =
    (
      ~bbox: Bobcat.Node.bbox,
      ~renderedElem: React.element,
      ~transform: Bobcat.Node.transform,
      ~nextTransform: Bobcat.Node.transform,
    ) => {
  let AnimationComponentHelper.{curr, next} = AnimationComponentHelper.useAnimation();

  let initPos =
    switch (curr) {
    | Before => (
        transform.translate.x,
        transform.translate.y,
        transform.scale.x,
        transform.scale.y,
      )
    | After => (
        nextTransform.translate.x,
        nextTransform.translate.y,
        nextTransform.scale.x,
        nextTransform.scale.y,
      )
    };

  let (values, setValues) =
    SpringHook.use(~config=Spring.config(~mass=1., ~tension=80., ~friction=20.), initPos);
  switch (next) {
  | Before =>
    setValues((
      transform.translate.x,
      transform.translate.y,
      transform.scale.x,
      transform.scale.y,
    ))
  | After =>
    setValues((
      nextTransform.translate.x,
      nextTransform.translate.y,
      nextTransform.scale.x,
      nextTransform.scale.y,
    ))
  };

  <G
    transform={values->SpringHook.interpolate(computeSVGTransformFlattened(bbox))}
    style={ReactDOMRe.Style.make(
      ~transform=values->SpringHook.interpolate(computeSVGTransformFlattened(bbox)),
      (),
    )}>
    renderedElem
  </G>;
};
