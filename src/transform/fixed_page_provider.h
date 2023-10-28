//
// Created by focus on 10/24/23.
//

#pragma once

#include <transform/page_provider.h>
#include <table/schema.h>

namespace sdb {

PageProviderPtr make_fixed_page_provider(SchemaPtr schema);

} // namespace sdb