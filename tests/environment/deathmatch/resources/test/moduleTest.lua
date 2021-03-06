local TEST_ELEMENTS = {
    Ped(0, 0, 0, 0),
    Ped(0, 5, 6, 78),
    Ped(0, 9, 14, 778),
}

TEST_ELEMENTS[2].dimension = 523

function returnFive()
    return 5
end

local TEST_FUNCTIONS = {
    {
        name = "test_simple",
        description = "Hello world test",
        expected = { "Yes!" },
    },
    {
        name = "test_simpleList",
        description = "List output test",
        expected = { "Sample string", -543, true, 5.4 },
    },
    {
        name = "test_echo",
        description = "Multi value echo test",
        input = { "Hello world", true, false, 123123, -7.6 },
        expected = { "Hello world", true, false, 123123, -7.6 },
    },
    {
        name = "test_echo",
        description = "Echo with nil test",
        input = { nil, false },
        expected = { nil, false },
    },
    {
        name = "test_echo",
        description = "Numbers echo test",
        input = { 76, 76.8, 76.777779 },
        expected = { 76, 76.8, 76.777779 },
    },
    {
        name = "test_isNumber",
        description = "Successful number test",
        input = { 523.432 },
        expected = { true },
    },
    {
        name = "test_isNumber",
        description = "Bad number test",
        input = { "it's a string" },
        expected = { false },
    },
    {
        name = "test_isString",
        description = "Successful string test",
        input = { "it's a string" },
        expected = { true },
    },
    {
        name = "test_isString",
        description = "'Number is string' test",
        input = { 657474 },
        expected = { true },
    },
    {
        name = "test_isString",
        description = "Bad string test",
        input = { TEST_ELEMENTS[1] },
        expected = { false },
    },
    {
        name = "test_echoElement",
        description = "Successful echo element",
        input = { TEST_ELEMENTS[1] },
        expected = { TEST_ELEMENTS[1] },
    },
    {
        name = "test_echoElement",
        description = "Successful echo root",
        input = { root },
        expected = { root },
    },
    {
        name = "test_echoElement",
        description = "Bad echo element",
        input = { "string" },
        expected = { false },
    },
    {
        name = "test_strictTypes",
        description = "Successful {bool, string, int} test",
        input = { true, "i am string", 657 },
        expected = { true },
    },
    {
        name = "test_strictTypes",
        description = "Successful {bool, string, int} test. Float is integer (c) Lua",
        input = { true, "i am string", 657.86 },
        expected = { true },
    },
    {
        name = "test_strictTypes",
        description = "Bad {bool, string, int} test. 1 is not bool (c) Lua",
        input = { 1, "string", 657.86 },
        expected = { false },
    },
    {
        name = "test_strictTypes",
        description = "Successful {bool, string, int} test. Number is string (c) Lua",
        input = { false, 564, 657.86 },
        expected = { true },
    },
    {
        name = "test_simpleTable",
        description = "Table hello world test",
        input = {},
        expected = { { {
                           name = "name",
                           surname = "surname"
                       } } },
    },
    {
        name = "test_callGetElementPosition",
        description = "Call getElementPosition",
        input = { TEST_ELEMENTS[2] },
        expected = {
            TEST_ELEMENTS[2]:getPosition().x,
            TEST_ELEMENTS[2]:getPosition().y,
            TEST_ELEMENTS[2]:getPosition().z,
        },
    },
    {
        name = "test_callGetElementPosition",
        description = "Call getElementPosition for multiple arguments",
        input = { TEST_ELEMENTS[2], TEST_ELEMENTS[3] },
        expected = {
            TEST_ELEMENTS[2]:getPosition().x,
            TEST_ELEMENTS[2]:getPosition().y,
            TEST_ELEMENTS[2]:getPosition().z,
            TEST_ELEMENTS[3]:getPosition().x,
            TEST_ELEMENTS[3]:getPosition().y,
            TEST_ELEMENTS[3]:getPosition().z,
        },
    },
    {
        name = "test_callElementGetDimensionMethod",
        description = "Call element:getDimension",
        input = { TEST_ELEMENTS[2] },
        expected = { TEST_ELEMENTS[2]:getDimension() },
    },
    {
        name = "test_pushFunction",
        description = "Call function after parsing and pushing (C++)",
        input = { returnFive },
        expected = { returnFive() },
    },
    {
        name = "test_advancedTable",
        description = "Create advanced table",
        input = {  },
        expected = {
            "start",
            {
                -1,
                {
                    [true] = "value",
                    keyOne = 6547
                },
                -3
            },
            7854,
            "stop"
        },
    },
    {
        name = "test_echo",
        description = "Successful echo simple table",
        input = { {
                      "TEST_ELEMENTS[2]",
                      "value",
                      6745,
                  } },
        expected = { {
                         "TEST_ELEMENTS[2]",
                         "value",
                         6745,
                     } },
    },
    {
        name = "test_echo",
        description = "Successful echo nested tables",
        input = { {
                      "TEST_ELEMENTS[2]",
                      {
                          [true] = "value",
                          keyOne = 6547,
                      },
                  } },
        expected = { {
                         "TEST_ELEMENTS[2]",
                         {
                             [true] = "value",
                             keyOne = 6547,
                         },
                     } },
    },
    {
        name = "test_tableToList",
        description = "Successful table (parsed as map in C++) to list",
        input = { { 0, 1, 2, 3, 4, 5 } },
        expected = { 0, 1, 2, 3, 4, 5 },
    },
    {
        name = "test_tableToList",
        description = "Table (parsed as map in C++) to list (bad)",
        input = { { 0, 1, 2, 3, 4, 5, key='value' } },
        expected = { false },
    },
    {
        name = "test_listToMap",
        description = "list to map",
        input = { },
        expected = { {
                         [1] = 53,
                         [2] = 42,
                         [3] = 24,
                         [4] = 74,
                         [5] = 81,
                         ["key"] = 876,
                     } },
    },
    {
        name = "test_constructors",
        description = "Successful constructor tests",
        input = { "string" },
        expected = {"string", "string", "string"},
    },
    {
        name = "test_checkGetArgumentsUnexpected",
        description = "Check getArguments exception LuaUnexpectedType",
        input = { "string" },
        expected = { true },
    },
    {
        name = "test_checkGetArgumentsBad",
        description = "Check getArguments exception LuaBadType",
        input = { returnFive },
        expected = { true },
    },
    {
        name = "test_checkGetArgumentsOutOfRange",
        description = "Check getArguments exception LuaOutOfRange",
        input = {  },
        expected = { true },
    },
    {
        name = "test_checkParseArgumentObject",
        description = "Check parseArgument for lua object",
        input = { TEST_ELEMENTS[1] },
        expected = { true },
    },
    {
        name = "test_callFunction",
        description = "Call global function",
        input = { "returnFive" },
        expected = { returnFive() },
    },
}

addEventHandler("onResourceStart", resourceRoot, function()
    iprint('===============[ TESTING DEV ]===============')
    outputDebugString(test_dev_status())

    iprint('===============[ TESTING START ]===============')

    for _, v in pairs(TEST_FUNCTIONS) do
        addTest(v.name)
    end

    for _, v in pairs(TEST_FUNCTIONS) do
        runTest(
                v.name,
                v.input or {},
                v.expected,
                v.description or ""
        )
    end

    testStatus()

    iprint('===============[ TESTING END ]===============')
end)