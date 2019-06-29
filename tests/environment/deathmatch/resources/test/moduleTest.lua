local TEST_ELEMENTS = {
    Ped(0,0,0,0),
    Ped(0,5,6,78),
}

local TEST_FUNCTIONS = {
    {
        name = "test_simple",
        description = "Hello world test",
        expected = {"Yes!"},
    },
    {
        name = "test_simpleList",
        description = "List output test",
        expected = {"Sample string", -543, true, 5.4},
    },
    {
        name = "test_echo",
        description = "Multi value echo test",
        input = {"Hello world", true, false, 123123, -7.6},
        expected = {"Hello world", true, false, 123123, -7.6},
    },
    {
        name = "test_echo",
        description = "Echo with nil test",
        input = {nil, false},
        expected = {nil, false},
    },
    {
        name = "test_echo",
        description = "Numbers echo test",
        input = {76, 76.8, 76.777779},
        expected = {76, 76.8, 76.777779},
    },
    {
        name = "test_isNumber",
        description = "Successful number test",
        input = {523.432},
        expected = {true},
    },
    {
        name = "test_isNumber",
        description = "Bad number test",
        input = {"it's a string"},
        expected = {false},
    },
    {
        name = "test_isString",
        description = "Successful string test",
        input = {"it's a string"},
        expected = {true},
    },
    {
        name = "test_isString",
        description = "'Number is string' test",
        input = {657474},
        expected = {true},
    },
    {
        name = "test_isString",
        description = "Bad string test",
        input = {TEST_ELEMENTS[1]},
        expected = {false},
    },
    {
        name = "test_echoElement",
        description = "Successful echo element",
        input = {TEST_ELEMENTS[1]},
        expected = {TEST_ELEMENTS[1]},
    },
    {
        name = "test_echoElement",
        description = "Successful echo root",
        input = {root},
        expected = {root},
    },
    {
        name = "test_echoElement",
        description = "Bad echo element",
        input = {"string"},
        expected = {false},
    },
    {
        name = "test_strictTypes",
        description = "Successful {bool, string, int} test",
        input = {true, "i am string", 657},
        expected = {true},
    },
    {
        name = "test_strictTypes",
        description = "Successful {bool, string, int} test. Float is integer (c) Lua",
        input = {true, "i am string", 657.86},
        expected = {true},
    },
    {
        name = "test_strictTypes",
        description = "Bad {bool, string, int} test. 1 is not bool (c) Lua",
        input = {1, "string", 657.86},
        expected = {false},
    },
    {
        name = "test_strictTypes",
        description = "Successful {bool, string, int} test. Number is string (c) Lua",
        input = {false, 564, 657.86},
        expected = {true},
    },
    {
        name = "test_simpleTable",
        description = "Table hello world test",
        input = {},
        expected = {{{
                        name="name",
                        surname="surname"
                    }}},
    },
    {
        name = "test_callGetElementPosition",
        description = "Call getElementPosition",
        input = {TEST_ELEMENTS[2]},
        expected = {
            TEST_ELEMENTS[2]:getPosition().x,
            TEST_ELEMENTS[2]:getPosition().y,
            TEST_ELEMENTS[2]:getPosition().z,
        },
    },
}

addEventHandler("onResourceStart", resourceRoot, function()
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