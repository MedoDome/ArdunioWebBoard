#See https://aka.ms/containerfastmode to understand how Visual Studio uses this Dockerfile to build your images for faster debugging.

FROM mcr.microsoft.com/dotnet/core/aspnet:3.1-buster-slim AS base
WORKDIR /app
EXPOSE 80

FROM mcr.microsoft.com/dotnet/core/sdk:3.1-buster AS build
WORKDIR /src
COPY ["MegaHeart/MegaHeart.csproj", "MegaHeart/"]
RUN dotnet restore "MegaHeart/MegaHeart.csproj"
COPY . .
WORKDIR "/src/MegaHeart"
RUN dotnet build "MegaHeart.csproj" -c Release -o /app/build

FROM build AS publish
RUN dotnet publish "MegaHeart.csproj" -c Release -o /app/publish

FROM base AS final
WORKDIR /app
COPY --from=publish /app/publish .
ENTRYPOINT ["dotnet", "MegaHeart.dll"]