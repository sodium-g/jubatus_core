// Jubatus: Online machine learning framework for distributed environment
// Copyright (C) 2013 Preferred Networks and Nippon Telegraph and Telephone Corporation.
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License version 2.1 as published by the Free Software Foundation.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

#include <string>
#include <gtest/gtest.h>
#include "jubatus/util/data/unordered_set.h"
#include "../common/exception.hpp"
#include "random_unlearner.hpp"
#include "test_util.hpp"

using jubatus::util::data::unordered_set;

namespace jubatus {
namespace core {
namespace unlearner {

TEST(random_unlearner, max_size_must_be_positive) {
  random_unlearner::config config;
  config.max_size = -1;
  EXPECT_THROW(random_unlearner unlearner(config), common::config_exception);
  config.max_size = 0;
  EXPECT_THROW(random_unlearner unlearner(config), common::config_exception);
}

TEST(random_unlearner, seed_must_positive) {
  random_unlearner::config config;
  config.max_size = 1;
  config.seed = -1;
  EXPECT_THROW(random_unlearner unlearner(config), common::config_exception);
}

TEST(random_unlearner, seed_must_be_within_32bit) {
  random_unlearner::config config;
  config.max_size = 1;
  config.seed = 0xffffffffLLU + 1;
  EXPECT_THROW(random_unlearner unlearner(config), common::config_exception);
}

TEST(random_unlearner, remove) {
  random_unlearner::config config;
  config.max_size = 3;
  random_unlearner unlearner(config);

  // add 3 IDs
  unlearner.touch("id1");
  unlearner.touch("id2");
  unlearner.touch("id3");

  // remove the middle ID
  EXPECT_TRUE(unlearner.remove("id2"));
  EXPECT_TRUE(unlearner.exists_in_memory("id1"));
  EXPECT_FALSE(unlearner.exists_in_memory("id2"));
  EXPECT_TRUE(unlearner.exists_in_memory("id3"));

  // add 1 ID
  EXPECT_TRUE(unlearner.touch("id4"));
  EXPECT_TRUE(unlearner.exists_in_memory("id1"));
  EXPECT_FALSE(unlearner.exists_in_memory("id2"));
  EXPECT_TRUE(unlearner.exists_in_memory("id3"));
  EXPECT_TRUE(unlearner.exists_in_memory("id4"));

  // remove the last ID
  EXPECT_TRUE(unlearner.remove("id4"));
  EXPECT_TRUE(unlearner.exists_in_memory("id1"));
  EXPECT_FALSE(unlearner.exists_in_memory("id2"));
  EXPECT_TRUE(unlearner.exists_in_memory("id3"));
  EXPECT_FALSE(unlearner.exists_in_memory("id4"));

  // remove non-existent ID
  EXPECT_FALSE(unlearner.remove("foo"));
  EXPECT_FALSE(unlearner.remove("id4"));
}

TEST(random_unlearner, trivial) {
  unordered_set<std::string> keys;
  keys.insert("id1");
  keys.insert("id2");
  keys.insert("id3");

  random_unlearner::config config;
  config.max_size = 3;
  random_unlearner unlearner(config);

  mock_callback callback;
  unlearner.set_callback(callback);

  for (unordered_set<std::string>::iterator it = keys.begin();
       it != keys.end(); ++it) {
    unlearner.touch(*it);
    EXPECT_EQ("", callback.unlearned_id());
    EXPECT_TRUE(unlearner.exists_in_memory(*it));
  }

  unlearner.touch("last");
  EXPECT_EQ(1ul, keys.count(callback.unlearned_id()));
  EXPECT_FALSE(unlearner.exists_in_memory(callback.unlearned_id()));
}

}  // namespace unlearner
}  // namespace core
}  // namespace jubatus
