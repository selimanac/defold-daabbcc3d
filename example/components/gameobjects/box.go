components {
  id: "model1"
  component: "/example/scripts/model.script"
}
embedded_components {
  id: "model"
  type: "model"
  data: "mesh: \"/builtins/assets/meshes/cube.dae\"\n"
  "name: \"{{NAME}}\"\n"
  "materials {\n"
  "  name: \"default\"\n"
  "  material: \"/builtins/materials/model.material\"\n"
  "  textures {\n"
  "    sampler: \"tex0\"\n"
  "    texture: \"/example/assets/images/aabb_40x40.png\"\n"
  "  }\n"
  "}\n"
  ""
}
