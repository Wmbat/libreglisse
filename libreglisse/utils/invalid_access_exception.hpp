/**
 *
 */

#ifndef LIBREGLISSE_UTILS_INVALID_ACCESS_EXCEPTION
#define LIBREGLISSE_UTILS_INVALID_ACCESS_EXCEPTION

#include <exception>
#include <string>
#include <utility>

namespace reglisse
{
   class invalid_access_exception : public std::exception
   {
   public:
      invalid_access_exception(std::string msg) : m_msg(std::move(msg)) {}

      [[nodiscard]] auto what() const noexcept -> const char* override { return m_msg.c_str(); }

   private:
      std::string m_msg;
   };
} // namespace reglisse

#endif // LIBREGLISSE_UTILS_INVALID_ACCESS_EXCEPTION
