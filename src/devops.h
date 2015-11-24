/**
 * KEEN.IO
 */
#define KEENIO_PROJECT_ID       getenv("KEENIO_PROJECT_ID")
#define KEENIO_WRITE_KEY        getenv("KEENIO_WRITE_KEY")
#define KEENIO_READ_KEY         getenv("KEENIO_READ_KEY")
#define KEENIO_API_KEY          getenv("KEENIO_API_KEY")

/**
 * PAGERDUTY
 */
#define PAGERDUTY_API_KEY       getenv("PAGERDUTY_API_KEY")

/**
 * GITHUB
 */
#define GITHUB_ACCESS_TOKEN   getenv("GITHUB_ACCESS_TOKEN")

#define GITHUB_REPO_NAME      "zachflower/oasis-mud"

#define GITHUB_LABEL_BUG      "bug"
#define GITHUB_LABEL_IDEA     "idea"
#define GITHUB_LABEL_TYPO     "typo"
#define GITHUB_LABEL_TODO     "todo"

/**
 * Curl Data Struct
 */
struct curl_data {
    size_t size;
    char* data;
};
