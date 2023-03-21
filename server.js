const express = require("express");
const app = express();
const bodyParser = require("body-parser");

app.use(bodyParser.json()); // to support JSON-encoded bodies
app.use(
    bodyParser.urlencoded({
        // to support URL-encoded bodies
        extended: true
    })
);

// Define the database: 
// Create mongodb database connection: 
// const dbURI = "mongodb://127.0.0.1/automatic-water-tank";

// Establish connection to database:
// mongoose
//     .connect(dbURI, {
//         useNewUrlParser: true,
//         useCreateIndex: true,
//         useUnifiedTopology: true,
//         useFindAndModify: true
//     })
//     .then(() => console.log("MongoDB connection successful"))
//     .catch(err => console.log(`MongoDB connection failed. Error: ${err}`));


// Define the REST API routes:
app.use("/api", require("./routes/index.js"));

const PORT = process.env.PORT || 3000;

app.listen(PORT, () => console.log(`Chaiful server app listening on port ${PORT}!`));