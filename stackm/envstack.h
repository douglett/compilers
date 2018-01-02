#include <string>
#include <cstdint>

typedef int32_t i32;

void lframe   ( );
void lunframe ( );
void ldef     (const std::string& id, i32 size);
i32  lgeta    (const std::string& id, i32 pos);
void lleta    (const std::string& id, i32 pos, i32 val);
i32  lget     (const std::string& id);
void llet     (const std::string& id, i32 val);
i32  loper    (const std::string& op, i32 v1, i32 v2);
std::string lgetstr(const std::string& id);