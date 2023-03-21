const express = require("express");
const router = express.Router();
const moment = require('moment');

// Params: 

// Variables: 
var temperature = 0.0

// Date object: 
let d = new Date();
var time_of_last_esp8266_ping = d.getTime();

// Variables: 
var wifi_ssid = "";

const sleep = (ms) =>
    new Promise(resolve => setTimeout(resolve, ms));


router.post("/esp8266data", (req, res) => {

    d = new Date();
    time_of_last_esp8266_ping = d.getTime();

    temperature = req.body.temperature;
    wifi_ssid = req.body.wifi_ssid;



    // Print to screen: 
    console.log(" Temperature is: ", temperature);
    console.log(" --------------------------------------- ");

    res.status(200).json({ message: " ESP8266 data received" });
});



router.get("/commands", (req, res) => {

});

router.get("/status", (req, res) => {

    res.status(200).json({
        temperature: Math.floor(temperature),
        wifi_ssid
    });

})


router.post("/commands", async (req, res) => {

});

module.exports = router;