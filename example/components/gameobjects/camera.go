components {
  id: "orbit_camera"
  component: "/example/scripts/lib/orbit_camera.script"
  properties {
    id: "target"
    value: ""
    type: PROPERTY_TYPE_HASH
  }
  properties {
    id: "distance"
    value: "30.0"
    type: PROPERTY_TYPE_NUMBER
  }
  properties {
    id: "distance_min"
    value: "0.0"
    type: PROPERTY_TYPE_NUMBER
  }
}
embedded_components {
  id: "camera"
  type: "camera"
  data: "aspect_ratio: 1.0\n"
  "fov: 0.7\n"
  "near_z: 0.1\n"
  "far_z: 100.0\n"
  "auto_aspect_ratio: 1\n"
  ""
}
