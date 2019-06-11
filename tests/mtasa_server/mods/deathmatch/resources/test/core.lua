TestsInfo = {
    total = 0,
    success = 0
}
Tests = {}

function addTest(name)
    Tests[name] = _G[name]
    iprint(Tests)
end

function checkTable(left, right)
    if #left ~= #right then
        return false
    end

    for i, v in ipairs(left) do
        if v ~= right[i] then
            return false
        end
    end
    return true
end

function runTest(name, excepted)
    TestsInfo.total = TestsInfo.total + 1
    iprint("Test " .. name)

    local result = {Tests[name]()}
    local status = checkTable(result, excepted)
    iprint(status, TestsInfo.success)
    TestsInfo.success = TestsInfo.success + (status and 1 or 0)
    iprint(status and "Test success" or "Test failed")
end

function testStatus()
    if TestsInfo.total == TestsInfo.success then
        iprint("[TEST TOTAL][OK] All tests passed!")
    else
        iprint("[TEST TOTAL][ER] Tests passed " .. TestsInfo.success .. "/" .. TestsInfo.total)
    end
end