module.exports = [
  {
    "type": "section",
    "items": [
      {
        "type": "heading",
        "defaultValue": "main colors"
      },
      {
        "type": "color",
        "messageKey": "m_fg",
        "defaultValue": "0xFFFFFF",
        "layout": "GRAY",
        "label": "font color"
      },
      {
        "type": "color",
        "layout": "GRAY",
        "messageKey": "m_bg",
        "defaultValue": "0x000000",
        "label": "background color"
      },
      {
        "type": "color",
        "capabilities": ["COLOR"],
        "messageKey": "r_fg",
        "defaultValue": "0x555555",
        "label": "random numbers color",
        "layout": [
          ["000000", "FFFFFF", "AAAAAA"],
          [false,    "555555", false]
        ]
      }
    ]
  },
  {
    "type": "section",
    "items": [
      {
        "type": "heading",
        "defaultValue": "time colors"
      },
      {
        "type": "color",
        "messageKey": "t_fg",
        "defaultValue": "0xFFFFFF",
        "layout": "GRAY",
        "label": "font color"
      },
      {
        "type": "color",
        "capabilities": ["COLOR"],
        "messageKey": "t_bg",
        "defaultValue": "0xFF55FF",
        "label": "background color"
      },
      {
        "type": "color",
        "capabilities": ["BW"],
        "layout": "GRAY",
        "messageKey": "t_bg",
        "defaultValue": "0x000000",
        "label": "background color"
      }
    ]
  },
  {
    "type": "submit",
    "defaultValue": "save"
  }
];
