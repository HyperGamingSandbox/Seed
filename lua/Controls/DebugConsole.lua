
local Control = require("Control")
local Edit = require("Controls\\Edit")
local RendererHost = require("DD\\Renderer\\Host")

local ConsoleWindow = Control.makeWrap(RBox)

local calcPos = function (o, w, h)
	-- o:set(#ROF_X, 0, 0)	
	o:set(#ROF_W, w, math.floor(h / 2))	
	o:cevent(#ROF_ONPARENTCHANGEPOS, o)
end

ConsoleWindow:addEvents({
	onAdd = function(self)
		-- dprint("onAdd")
		calcPos(self, RendererHost.getViewSize())
	end,
	onViewSize = function(self, w, h)
		-- dprint("onViewSize")
		calcPos(self, w, h)
	end
})

local DebugConsole = Control:extend()

function DebugConsole:initialize()

	
	dprint("DebugConsole:initialize")
	self.consoleWindow = self:add(ConsoleWindow:new(0, 0, 300, 300, 0, 1, 0xffffff))

	self.edit = self.consoleWindow:add(Edit:new(5, 20))
	self.asd = "zxc"

end

function DebugConsole:onDelete()
	dprint("DebugConsole:onDelete " .. self.asd)
end

return DebugConsole