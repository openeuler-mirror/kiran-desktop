/**
 * Copyright (c) 2020 ~ 2024 KylinSec Co., Ltd.
 * network is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PSL v2 for more details.
 *
 * Author:     liuxinhao <liuxinhao@kylinsec.com.cn>
 */
#pragma once
#include <QDebug>

#define BREAK_IF_FALSE(cond) \
    {                        \
        if (!(cond))         \
            break;           \
    }

#define BREAK_IF_TRUE(cond) \
    {                       \
        if (cond)           \
            break;          \
    }

#define RETURN_VAL_IF_FALSE(cond, val) \
    {                                  \
        if (!(cond))                   \
        {                              \
            return val;                \
        }                              \
    }
#define RETURN_VAL_IF_FALSE_WITH_WARNNING(cond, val, warnning) \
    {                                                          \
        if (!(cond))                                           \
        {                                                      \
            qWarning() << warnning;                            \
            return val;                                        \
        }                                                      \
    }

#define RETURN_VAL_IF_TRUE(cond, val) \
    {                                 \
        if (cond)                     \
            return val;               \
    }

#define RETURN_IF_FALSE(cond)                      \
    {                                              \
        if (!(cond))                               \
        {                                          \
            qDebug() << "The condition is false."; \
            return;                                \
        }                                          \
    }
#define RETURN_IF_FALSE_WITH_WARNNING(cond, warnning) \
    {                                                 \
        if (!(cond))                                  \
        {                                             \
            qWarning() << warnning;                   \
            return;                                   \
        }                                             \
    }

#define RETURN_IF_TRUE(cond) \
    {                        \
        if (cond)            \
            return;          \
    }

#define CONTINUE_IF_FALSE(cond) \
    {                           \
        if (!(cond))            \
            continue;           \
    }

#define CONTINUE_IF_TRUE(cond) \
    {                          \
        if (cond)              \
            continue;          \
    }
