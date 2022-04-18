


---@class ScriptConnection
---@param _signal Signal
---@param _func function
---@param _next ScriptConnection
---@param _connected boolean
local Connection = {}
Connection.__class = "ScriptConnection";
Connection.__index = Connection;

function Connection.new(signal, func)
    local self = setmetatable({}, Connection);

    self._connected = true;
    self._signal = signal;
    self._func = func;
    self._next = nil;

    return self;
end

function Connection:Disconnect()
    assert(self._connected);
    self._connected = false;

    if self._signal._header == self then
        self._signal._header = self._next;
    else
        local prev = self._signal._header;
        
		while prev and prev._next ~= self do
			prev = prev._next;
		end

		if prev then
			prev._next = self._next;
		end
    end
end

return Connection;