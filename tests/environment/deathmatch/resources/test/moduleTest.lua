local TEST_ELEMENTS = {
    Ped(0,0,0,0),
}

local TEST_FUNCTIONS = {
    {
        name = "test_simple",
        expected = {"Yes!"},
    },
    {
        name = "test_simpleList",
        expected = {"Sample string", -543, true, 5.4},
    },
    {
        name = "test_echo",
        input = {"Hello world", true, false, 123123, -7.6},
        expected = {"Hello world", true, false, 123123, -7.6},
    },
    {
        name = "test_isNumber",
        input = {523.432},
        expected = {true},
    },
    {
        name = "test_isNumber",
        input = {"it's a string"},
        expected = {false},
    },
    {
        name = "test_isString",
        input = {"it's a string"},
        expected = {true},
    },
    {
        name = "test_isString",
        input = {TEST_ELEMENTS[1]},
        expected = {false},
    },
    {
        name = "test_echoElement",
        input = {TEST_ELEMENTS[1]},
        expected = {TEST_ELEMENTS[1]},
    },
    {
        name = "test_echoElement",
        input = {root},
        expected = {root},
    },
    {
        name = "test_simpleTable",
        input = {},
        expected = {{{
                        name="name",
                        surname="surnameaaaa"
                    }}},
    }
}

addEventHandler("onResourceStart", resourceRoot, function()
    for _, v in pairs(TEST_FUNCTIONS) do
        addTest(v.name)
    end

    for _, v in pairs(TEST_FUNCTIONS) do
        runTest(v.name, v.input or {}, v.expected)
    end

    testStatus()
end)