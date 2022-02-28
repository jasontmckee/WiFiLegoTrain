const express = require('express')
const multer  = require('multer')
const app = express()
const port = 3000
const postForm = multer()

app.use(express.static('../Train/html'))

app.get('/api/config', (req, res) => {
  res.json(config)
})

app.post('/api/config', postForm.none(), function(req, res) {
  // copy tags into proper format
  for(var i=0; i<req.body.tagName.length; i++) {
    config.tags[i].tagName = req.body.tagName[i]
    config.tags[i].tag = req.body.tag[i]
  }
  // remove tag/tagName arrays and copy the rest of the fields to config
  delete req.body.tag
  delete req.body.tagName
  Object.assign(config,req.body)

  res.json(config)
})

app.post('/api/control', postForm.none(), (req, res) => {
  console.log(req.body)
  if(req.body.rfid == "next") req.body.rfid = config.tags[0].tag
  req.body.battery = 84
  res.json(req.body)
})

app.listen(port, () => {
  console.log(`SoftTrain started on port ${port}`)
})

// firmware config responses object
var config = {
  "configVersion": 7,
  "buildTimestamp": "Feb 19 2022 12:39:52",
  "name": "Train 1",
  "batteryEmpty": 475,
  "batteryFull": 913,
  "motorMin": 95,
  "motorMax": 135,
  "activeBrake": true,
  "brakeTimeMS": 200,
  "beep": true,
  "beepMS": 250,
  "displayRaw": false,
  "dynamicPower": true,
  "tags": [
    {
      "tagName": "2",
      "tag": "69b3d9d9"
    },
    {
      "tagName": "4",
      "tag": "2ecbd9d9"
    },
    {
      "tagName": "7",
      "tag": "de2adbd9"
    },
    {
      "tagName": "",
      "tag": ""
    },
    {
      "tagName": "",
      "tag": ""
    },
    {
      "tagName": "",
      "tag": ""
    },
    {
      "tagName": "",
      "tag": ""
    },
    {
      "tagName": "",
      "tag": ""
    },
    {
      "tagName": "",
      "tag": ""
    },
    {
      "tagName": "",
      "tag": ""
    }
  ]
};
