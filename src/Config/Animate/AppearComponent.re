/* rendering for nodes that appear b/c they did not have flow */
open SpringHelper;

module SpringHook =
  Spring.MakeSpring({
    type t = float;
    type interpolate = float => float;
  });

[@react.component]
let make = (~renderedElem: React.element) => {
  let AnimationComponentHelper.{curr, next} = AnimationComponentHelper.useAnimation();

  let initPos =
    switch (curr) {
    | Before => 0.
    | After => 1.
    };

  let (values, setValues) =
    /* duration = 0.1s, damping = 100% */
    SpringHook.use(~config=Spring.config(~mass=1., ~tension=4406.77, ~friction=132.77), initPos);
  switch (next) {
  | Before => setValues(0.)
  | After => setValues(1.)
  };

  <G style={ReactDOMRe.Style.make(~opacity=values->SpringHook.interpolate(x => x), ())}>
    renderedElem
  </G>;
};
