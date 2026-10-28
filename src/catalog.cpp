// Copyright (c) 2026 Samsung Electronics Co., Ltd. SPDX-License-Identifier: Apache-2.0
#include "catalog.h"
#include <libintl.h>
#include <stdexcept>

namespace Buxby
{
Catalog::Catalog(const char* domain)
: mDomain(domain)
{
  if(!bindtextdomain(mDomain, BUXBY_LOCALE_DIR) || !bind_textdomain_codeset(mDomain, "UTF-8"))
  {
    throw std::runtime_error("Could not bind the Buxby gettext catalog");
  }
}

const char* Catalog::Get(const char* messageId) const
{
  return dgettext(mDomain, messageId);
}
}
