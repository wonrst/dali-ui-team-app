// Copyright (c) 2026 Samsung Electronics Co., Ltd. SPDX-License-Identifier: Apache-2.0
#pragma once

namespace Buxby
{
// Each UI member owns a gettext domain. Appfw owns the process locale.
class Catalog
{
public:
  explicit Catalog(const char* domain);
  const char* Get(const char* messageId) const;

private:
  const char* mDomain;
};
}
