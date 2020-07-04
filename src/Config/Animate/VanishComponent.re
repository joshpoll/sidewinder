/* rendering for nodes that vanish b/c they did not have flow */
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
    | Before => 1.
    | After => 0.
    };

  let (values, setValues) =
    /* duration = 0.1s, damping = 100% */
    SpringHook.use(~config=Spring.config(~mass=1., ~tension=4406.77, ~friction=132.77), initPos);
  switch (next) {
  | Before => setValues(1.)
  | After => setValues(0.)
  };

  <G style={ReactDOMRe.Style.make(~opacity=values->SpringHook.interpolate(x => x), ())}>
    renderedElem
  </G>;
};
