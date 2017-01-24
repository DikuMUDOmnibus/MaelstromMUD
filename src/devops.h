/**
 * GITHUB
 */
#define GITHUB_ACCESS_TOKEN       getenv("GITHUB_ACCESS_TOKEN")

#define GITHUB_REPO_NAME          "zachflower/oasis-mud"

#define GITHUB_LABEL_BUG          "bug"
#define GITHUB_LABEL_IDEA         "idea"
#define GITHUB_LABEL_TYPO         "typo"
#define GITHUB_LABEL_TODO         "todo"

/**
 * NETUITIVE
 */
#define NETUITIVE_USERNAME        "zach@zacharyflower.com"
#define NETUITIVE_PASSWORD        getenv("NETUITIVE_PASSWORD")

#define NETUITIVE_API_KEY         getenv("NETUITIVE_API_KEY")
#define NETUITIVE_ELEMENT_ID      "OASIS MUD"

#define NETUITIVE_LEVEL_INFO      "INFO"
#define NETUITIVE_LEVEL_WARNING   "WARNING"
#define NETUITIVE_LEVEL_CRITICAL  "CRITICAL"


/**
 * Curl Data Struct
 */
struct curl_data {
    size_t size;
    char* data;
};
