// Entry point

[@bs.val] external document: Js.t({..}) = "document";

// We're using raw DOM manipulations here, to avoid making you read
// ReasonReact when you might precisely be trying to learn it for the first
// time through the examples later.
let style = document##createElement("style");
document##head##appendChild(style);
style##innerHTML #= ExampleStyles.style;

let makeContainer = text => {
  let container = document##createElement("div");
  container##className #= "container";

  let title = document##createElement("div");
  title##className #= "containerTitle";
  title##innerText #= text;

  let content = document##createElement("div");
  content##className #= "containerContent";

  let () = container##appendChild(title);
  let () = container##appendChild(content);
  let () = document##body##appendChild(container);

  content;
};

/* let flows: list(Flow.linear) = [[("x", ["x"]), ("y", ["y"])], []];
   let nodes: list(ConfigIR.node) = [AnimationExamples.transition0, AnimationExamples.transition1];

   let configs =
     List.map(
       ((flow, node)) => Sidewinder.Config.propagatePlace(flow, node),
       List.combine(flows, nodes),
     );

   ReactDOMRe.render(<AnimationTester trace=configs />, makeContainer("Transition Animation"));

   let flows: list(Flow.linear) = [[("x", []), ("y", ["y"])], []];
   let nodes: list(ConfigIR.node) = [AnimationExamples.transition0, AnimationExamples.delete1];

   let configs =
     List.map(
       ((flow, node)) => Sidewinder.Config.propagatePlace(flow, node),
       List.combine(flows, nodes),
     );

   ReactDOMRe.render(<AnimationTester trace=configs />, makeContainer("Deletion Animation"));

   let flows: list(Flow.linear) = [[("x", ["x"]), ("y0", []), ("y1", ["y1"])], []];
   let nodes: list(ConfigIR.node) = [AnimationExamples.nested0, AnimationExamples.nested1];

   let configs =
     List.map(
       ((flow, node)) => Sidewinder.Config.propagatePlace(flow, node),
       List.combine(flows, nodes),
     );

   ReactDOMRe.render(<AnimationTester trace=configs />, makeContainer("Nested Animation"));

   let flows: list(Flow.linear) = [[("box", ["box"]), ("y", ["y"])], []];
   let nodes: list(ConfigIR.node) = [AnimationExamples.boxed0, AnimationExamples.boxed1];

   let configs =
     List.map(
       ((flow, node)) => Sidewinder.Config.propagatePlace(flow, node),
       List.combine(flows, nodes),
     );

   Js.log(configs |> Array.of_list);

   ReactDOMRe.render(<AnimationTester trace=configs />, makeContainer("Boxed Animation"));

   let flows: list(Flow.linear) = [[]];
   let nodes: list(ConfigIR.node) = [TransformationExamples.add];

   let configs =
     List.map(
       ((flow, node)) => Sidewinder.Config.propagatePlace(flow, node),
       List.combine(flows, nodes),
     );

   Js.log(configs |> Array.of_list);

   ReactDOMRe.render(<AnimationTester trace=configs />, makeContainer("Transformation")); */
