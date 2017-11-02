/**
 * GITHUB
 */
#define GITHUB_ACCESS_TOKEN   getenv("GITHUB_ACCESS_TOKEN")

#define GITHUB_REPO_NAME      "zachflower/maelstrom-mud"

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
