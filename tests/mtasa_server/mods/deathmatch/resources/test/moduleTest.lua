local TEST_FUNCTIONS = {
    {
        name = "moduleSdkTest1",
        expected = {"Yesn't!"}
    },
}

addEventHandler("onResourceStart", resourceRoot, function()
    for _, v in pairs(TEST_FUNCTIONS) do
        addTest(v.name)
    end

    for _, v in pairs(TEST_FUNCTIONS) do
        runTest(v.name, v.expected)
    end

    testStatus()
end)