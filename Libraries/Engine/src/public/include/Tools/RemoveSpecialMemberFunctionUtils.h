#pragma once
namespace Engine
{
	class IRemoveCopies
	{
	public:
		IRemoveCopies() = default;
		IRemoveCopies(const IRemoveCopies& copy) = delete;
		IRemoveCopies& operator=(const IRemoveCopies& copy) = delete;
	};
	using RemoveCopies = IRemoveCopies; //added for easy porting.

	class IRemoveMoves
	{
	public:
		IRemoveMoves() = default;
		IRemoveMoves(IRemoveMoves&& move) = delete;
		IRemoveMoves& operator=(IRemoveMoves&& move) = delete;
	};
	using RemoveMoves = IRemoveMoves; //added for easy porting.
}