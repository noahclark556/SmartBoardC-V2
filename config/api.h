// These are the only API configurations you need to change for the entire project. 
// API related changes are no longer required for the voice daemon, these are passed to it.

// GPT-4 enabled API key
#define OPENAI_API_KEY "OPENAI_API_KEY"
// Ensure this key is linked to a project with the Google Cloud Text-to-Speech API enabled
#define GOOGLE_API_KEY "GOOGLE_API_KEY"
// Cloud function to read data from the database (see README.md for more info)
#define READ_DB_FUNCTION_URL "https://my-firebase-project.cloudfunctions.net/getSmartBoardData" 
// Cloud function to update the database with the user's response (see README.md for more info)
#define UPDATE_DB_FUNCTION_URL "https://my-firebase-project.cloudfunctions.net/updateSmartBoardHistory"
// User ID for the database (matches document in firebase collection. See README.md for more info)
#define DB_USER_ID "my-user-id"
// News API URL, built to use the NewsData.org API. Make sure to add your own API key.
#define NEWS_API_URL "https://newsdata.io/api/1/latest?apikey=YOUR_API_KEY&country=us&size=3&language=en"
// Cloud function to update the agenda (see README.md for more info)
#define UPDATE_AGENDA_FUNCTION_URL "https://my-firebase-project.cloudfunctions.net/updateSmartBoardAgenda"